import * as mqtt from 'mqtt';
import { logger } from "../../infrastructure/loggers";
// import des fonctions pour les noeuds, véhicules, etc. si nécessaire
import { Nodes } from "../entities/Nodes";
import { Origins } from '../entities/Origins';
import { Alarms } from '../entities/Alarms';
import { AlarmsTypes } from '../entities/AlarmsTypes';
import { Arcs } from '../entities/Arcs';
import { AppDataSource } from "../../infrastructure/database/AppDataSource";


export interface MqttRequest {
  commandId: string // Id de la commande (actuellement action + timestamp)
  action: string // le type de la requête (GET_MAP_REQUEST, SET_WAYPOINTS)
  replyTopic: string // Le topic qui attend la réponse
  // Le reste c'est les paramètres de la requête (j'avais la flemme de faire un attr en plus mais ça peut se faire si il faut) 
}

export interface MqttAlerts
{
  origin: string // Service à l'origine du message
  timestamp: number // Timestamp en seconde
  level: string // Niveau de log : DEBUG, INFO, WARNING, ERROR, FATAL
  message: string // Message du log
}

export class MqttClientService {
    private client: mqtt.MqttClient;
    private topicList: string[] = ['services/api/request', 'services/api/response', 'system/alerts'];
    private messageHandlers: { [topic: string]: (topic: string, message: Buffer) => void } = {};
    private clientId: string = 'mqtt_client_' + Math.random().toString(16).substr(2, 8);
    private username: string = "";
    private password: string = "";
    private brokerUrl: string = 'mqtt://10.152.29.106';

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
        
                this.onMessage('services/api/response', (topic, message) => {
                    logger.info(`Response received on ${topic}: ${message.toString()}`);
                    
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

    registerAlert(): void {
    }

    disconnect(): void {
        this.client.end();
    }
}
