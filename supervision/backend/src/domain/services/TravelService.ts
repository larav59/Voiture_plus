import { Repository, Like, LessThan, MoreThan } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Travels } from "../entities/Travels";
import { Nodes } from "../entities/Nodes";
import { Vehicles } from "../entities/Vehicles";
import { Users } from "../entities/Users";
import { TravelsNodes } from "../entities/TravelsNodes";

export class TravelService {
	private travelRepository: Repository<Travels>;
	private nodesRepository: Repository<Nodes>;
	private travelNodesRepository: Repository<TravelsNodes>;
	private vehicleRepository: Repository<Vehicles>;
	private userRepository: Repository<Users>;

	constructor() {
		this.travelRepository = AppDataSource.getRepository(Travels);
		this.travelNodesRepository = AppDataSource.getRepository(TravelsNodes);
		this.vehicleRepository = AppDataSource.getRepository(Vehicles);
		this.userRepository = AppDataSource.getRepository(Users);
		this.nodesRepository = AppDataSource.getRepository(Nodes);
	}

	// Méthode pour récupérer les origines
	async getTravels(vehicle : number, createdBefore: Date | "" , createdAfter: Date | "", status: string | null): Promise<Travels[]> {
		const query = this.travelRepository.find({
			relations: ['travelsNodes', 'travelsNodes.node','travelsNodes.node.nodeType', 'vehicle', 'vehicle.states'],
			where: {
				...(vehicle ? { vehicle: Like(`%${vehicle}%`) } : {}),
				...(createdBefore ? { createdAt: LessThan(createdBefore) } : {}),
				...(createdAfter? { createdAt: MoreThan(createdAfter) } : {}),
				...(status? { status: Like(`%${status}%`) } : {})
			},
			order : { travelsNodes : { order : "ASC"} }
		});
		return query;
	}

	async createTravel(vehicle: Number | null, nodes : any[] | null, user : string): Promise<Travels> {
		const vehicleId = await this.vehicleRepository.findOneBy({id: vehicle !== null ? Number(vehicle) : 0});
		const userEntity = await this.userRepository.findOneBy({username: user});
		const nodeList : Nodes[] = [];
		if (vehicle !== null && !vehicleId) {
			throw new Error("Véhicule non trouvé");
		}
		for (const node of nodes !== null ? nodes : []) {
			const Node = await this.nodesRepository.findOneBy({id: node.id});
			if (!Node) throw new Error(`Noeud de voyage non trouvé: ${node.id}`);
			nodeList.push(Node);
		}
		const newTravel = new Travels({
			vehicleId: vehicle !== null ? Number(vehicle) : 0,
			createdAt: new Date(),
			status: "En attente",
			estimatedDuration: null,
			orderedBy: userEntity?.username?? "",
			travelsNodes: []
		});
		await this.travelRepository.save(newTravel);
		let i=0;
		for (const node of nodeList) {
			const travelNode = new TravelsNodes({
				travelId: newTravel.id,
				nodeId: node.id,
				order: nodes![i].order ?? i,
			});
			newTravel.travelsNodes.push(travelNode);
			i++;
			await this.travelNodesRepository.save(travelNode);	
		};
		await this.travelRepository.save(newTravel);
		// return the saved travel with relations
		return this.travelRepository.findOne({
			where: { id: newTravel.id },
			relations: ['travelsNodes', 'travelsNodes.node','travelsNodes.node.nodeType', 'vehicle', 'vehicle.states']
		}) as Promise<Travels>;
		
	}

	async updateTravel(id: number, status: string , estimated_duration: number): Promise<Travels | null> {
		const travel = await this.travelRepository.findOneBy({ id });
		if (!travel) {
			return null;
		}
		travel.status = status;
		travel.estimatedDuration = estimated_duration;
		this.travelRepository.save(travel);
		return this.travelRepository.findOne({
			where: { id: travel.id },
			relations: ['travelsNodes', 'travelsNodes.node','travelsNodes.node.nodeType', 'vehicle', 'vehicle.states']
		}) as Promise<Travels>;
	}
}

