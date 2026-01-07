import * as mqtt from 'mqtt';
import { logger } from "../../infrastructure/loggers";
// import des fonctions pour les noeuds, véhicules, etc. si nécessaire
import { Nodes } from "../entities/Nodes";
import { Origins } from '../entities/Origins';
import { Alarms } from '../entities/Alarms';
import { AlarmsTypes } from '../entities/AlarmsTypes';
import { States } from '../entities/States';
import { Arcs } from '../entities/Arcs';
import { Vehicles } from '../entities/Vehicles';
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Travels } from '../entities/Travels';


export interface MqttRequest {
  commandId: string // Id de la commande (actuellement action + timestamp)
  action: string // le type de la requête (GET_MAP_REQUEST, SET_WAYPOINTS)
  replyTopic: string // Le topic qui attend la réponse
}

export interface MqttTravelRequest extends MqttRequest {
    carId: number
    nodeList: number[]
    replyTopic: string
    commandId: string
    action: string
}

export interface MqttResponse {
    commandId: string
    status: string // "OK" si la commande a réussi, "ERROR" sinon 
    message?: string // Message d'erreur si status est "ERROR"
}

export interface MqttAlerts
{
  origin: string // Service à l'origine du message
  timestamp: number // Timestamp en seconde
  level: string // Niveau de log : DEBUG, INFO, WARNING, ERROR, FATAL
  message: string // Message du log
}

export interface MqttStateUpdate
{
    carId: number
    timestamp: number // Timestamp en seconde
    x: number // Position X du véhicule
    y: number // Position Y du véhicule
    angle: number // Orientation du véhicule en degré
    speed: number // Vitesse du véhicule en m/s
    isNavigating: boolean // trajet en cours ?
    obstacleDetected: boolean // obstacle detecté ?
}

export class MqttClientService {
    private client: mqtt.MqttClient;
    private topicList: string[] = ['services/api/request', 'services/api/response', 'system/alerts'];
    private messageHandlers: { [topic: string]: (topic: string, message: Buffer) => void } = {};
    private clientId: string = 'mqtt_client_' + Math.random().toString(16).substr(2, 8);
    private username: string = "";
    private password: string = "";
    private brokerUrl: string = 'mqtt://10.57.224.106';
    private static mqttQueue: any[] = [] ;

    constructor(username?: string, password?: string, clientId?: string, brokerUrl?: string) {
        if (username) this.username = username;
        if (password) this.password = password;
        if (clientId) this.clientId = clientId;
        if (brokerUrl) this.brokerUrl = brokerUrl;
        this.client = mqtt.connect(this.brokerUrl, {
            clean: true,
            connectTimeout: 4000,
            clientId: this.clientId,
            username: this.username,
            password: this.password,
        });

        this.client.on('connect', () => {
            console.log('Connected to MQTT broker');
        });

        this.client.on('error', (error) => {
            console.error('MQTT Connection Error:', error);
        });

                this.onMessage('services/api/request', (topic, message) => {
                    logger.info(`Message received on ${topic}: ${message.toString()}`);
                    // message is a json string containing commandId and message
                    const request: MqttRequest = JSON.parse(message.toString());
        
                    logger.info(`Processing commandId: ${request.commandId}, action: ${request.action}`);

                    switch (request.action) {
                        case 'GET_MAP_REQUEST':
                            // Here you would typically call a service to get the nodes
                            const nodesQuery = AppDataSource.getRepository(Nodes).find({relations: ['nodeType']});
                            const arcsQuery = AppDataSource.getRepository(Arcs).find({relations: ['originNode', 'destinationNode']});
                            Promise.all([nodesQuery, arcsQuery]).then(([nodes, arcs]) => {
                                this.publish(request.replyTopic, JSON.stringify({
                                    commandId: request.commandId,
                                    status: 'true',
                                    timestampSec: Math.floor(Date.now() / 1000),
                                    timestampNsec: (Date.now() % 1000) * 1e6,
                                    nodes : nodes.map((node) => ({
                                        id: node.id,
                                        type: node.nodeTypeId,
                                        x: node.positionX,
                                        y: node.positionY,
                                        edges: arcs.filter(arc => arc.originNode.id === node.id).map(arc => ({
                                            target: arc.destinationNode.id,
                                            weight: arc.weight,
                                            rule: arc.type
                                        }))
                                    }))
                                }));
                                logger.info(`Map data sent for commandId: ${request.commandId}`);
                            }).catch((error) => {
                                logger.error(`Error fetching map data: ${error.message}`);
                                this.publish(request.replyTopic, JSON.stringify({
                                    commandId: request.commandId,
                                    status: 'ERROR',
                                    message: `Error fetching map data: ${error.message}`
                                }));
                            });
                        break;                        

                        default:
                            logger.error(`Unknown action: ${request.action}`);
                            this.publish(request.replyTopic, JSON.stringify({
                                commandId: request.commandId,
                                status: 'ERROR',
                                message: `Unknown action: ${request.action}`
                            }));
                        break;
                    }
                });

                const vehicules =  AppDataSource.getRepository(Vehicles).find();
                vehicules.then((vehicles) => {
                    vehicles.forEach((vehicle) => {
                        this.onMessage(`vehicles/${vehicle.id}/state`, (topic, message) => {
                            //logger.info(`State update for vehicle ${vehicle.id} on topic ${topic}: ${message.toString()}`);
                            const state: MqttStateUpdate = JSON.parse(message.toString());
                            const newState = AppDataSource.getRepository(States).create({
                                vehicleId : state.carId,
                                occuredAt : state.timestamp,
                                positionX : state.x,
                                positionY : state.y,
                                angle : state.angle,
                                speed : state.speed
                            });
                            AppDataSource.getRepository(States).save(newState);

                            if (!state.isNavigating) {
                                // get vehicle last travel 
                                const lastTravel = AppDataSource.getRepository(Travels).findOne({
                                    where: { vehicleId: state.carId, status: 'En cours' },
                                    order: { createdAt: "DESC" },
                                });
                                if (lastTravel) {
                                    lastTravel.then((travel) => {
                                        if (travel) {
                                            travel.status = 'Terminé';
                                            AppDataSource.getRepository(Travels).save(travel);
                                        }
                                    });
                                } 
                            }

                        });
                    });
                })
        
                this.onMessage('system/alerts', async (topic, message) => {
                    const alert: MqttAlerts = JSON.parse(message.toString());
                    logger.info(`ALERT from ${alert.origin} at ${new Date(alert.timestamp * 1000).toISOString()} [${alert.level}]: ${alert.message}`);
                    const type = await AppDataSource.getRepository(AlarmsTypes).findOneBy({ criticity : alert.level });
                    const origin = await AppDataSource.getRepository(Origins).findOneBy({ label : alert.origin });
                    const newAlarm = AppDataSource.getRepository(Alarms).create({
                        description: alert.message,
                        alarmTypeId: type?.id ?? null,
                        originId: origin?.id ?? null,
                        createdAt: new Date()
                    });
		            AppDataSource.getRepository(Alarms).save(newAlarm);
                });
        
                this.onMessage('services/api/response', async (topic, message) => {
                    logger.info(`Response received on ${topic}: ${message.toString()}`);
                    const response: MqttResponse = JSON.parse(message.toString());
                    logger.info(`Response for commandId: ${response.commandId}, status: ${response.status}, message: ${response.message ?? 'N/A'}`);
                    if (response.status === 'OK') {
                        logger.info(`Command ${response.commandId} executed successfully.`);
                        //check if commandid like   REQ_PLAN_VehicleId
                        if (response.commandId.startsWith('REQ_PLAN_')) {
                            const requestIndex = MqttClientService.mqttQueue.findIndex(req => req.commandId === response.commandId);
                            if (requestIndex !== -1) {
                                const request = MqttClientService.mqttQueue[requestIndex] as MqttTravelRequest;
                                const vehicleId = request.carId;
                                if (!isNaN(vehicleId)) {
                                    const travel = await AppDataSource.getRepository(Travels).findOne({
                                        where: { vehicleId: vehicleId, status: 'En attente' },
                                        order: { createdAt: "DESC" },
                                    });
                                    if (travel) {
                                        travel.status = 'En cours';
                                        await AppDataSource.getRepository(Travels).save(travel);
                                        logger.info(`Travel ${travel.id} for vehicle ${vehicleId} set to 'En cours'`);
                                    } else {
                                        logger.error(`No pending travel found for vehicleId: ${vehicleId}`);
                                    }
                                    // remove from queue
                                    MqttClientService.mqttQueue.splice(requestIndex, 1);
                                } else {
                                    logger.error(`Invalid vehicleId extracted from commandId: ${response.commandId}`);
                                }
                            }
                        }                     
                    } else {
                        logger.error(`Command ${response.commandId} failed with message: ${response.message}`);
                    }
                });

                this.subscribeToTopics();
        
    }

    publish(topic: string, message: string): void {
        this.client.publish(topic, message, (err) => {
            if (err) {
                console.error('Publish error:', err);
            }
        });
    }

    subscribeToTopics(): void {
        this.topicList.forEach((topic) => {
            this.client.subscribe(topic, (err) => {
                console.log(`Subscribed to topic: ${topic}`);
                if (err) {
                    console.error(`Subscribe error for topic ${topic}:`, err);
                }
            });
        });

        this.client.on('message', (topic, message) => {
            const handler = this.messageHandlers[topic];
            if (handler) {
                handler(topic, message);
            }
        });
    }

    onMessage(topic: string, handler: (topic: string, message: Buffer) => void): void {
        this.messageHandlers[topic] = handler;
    }
    
    subscribe(topic: string, messageHandler: (topic: string, message: Buffer) => void): void {
        this.client.subscribe(topic, (err) => {
            if (err) {
                console.error('Subscribe error:', err);
            }
        });

        this.client.on('message', (recvTopic, message) => {
            if (recvTopic === topic) {
                messageHandler(recvTopic, message);
            }
        });
    }

    addToQueue(request: MqttRequest): void {
        MqttClientService.mqttQueue.push(request);
    }

    registerAlert(): void {
    }

    disconnect(): void {
        this.client.end();
    }
}
