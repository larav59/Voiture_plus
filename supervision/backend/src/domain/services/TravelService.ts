import { Repository, Like, LessThanOrEqual, MoreThanOrEqual } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Travels } from "../entities/Travels";
import { TravelsNodes } from "../entities/TravelsNodes";
import { Vehicles } from "../entities/Vehicles";
import { Users } from "../entities/Users";

export class TravelService {
	private travelRepository: Repository<Travels>;
	private travelNodesRepository: Repository<TravelsNodes>;
	private vehicleRepository: Repository<Vehicles>;
	private userRepository: Repository<Users>;

	constructor() {
		this.travelRepository = AppDataSource.getRepository(Travels);
		this.travelNodesRepository = AppDataSource.getRepository(TravelsNodes);
		this.vehicleRepository = AppDataSource.getRepository(Vehicles);
		this.userRepository = AppDataSource.getRepository(Users);
	}

	// Méthode pour récupérer les origines
	async getTravels(vehicle : number, createdBefore: Date | "" , createdAfter: Date | "", status: string | null): Promise<Travels[]> {
		const query = this.travelRepository.find({
			relations: ['travelsNodes'],
			where: {
				...(vehicle ? { vehicle: Like(`%${vehicle}%`) } : {}),
				...(createdBefore ? { createdAt: LessThanOrEqual(createdBefore) } : {}),
				...(createdAfter? { createdAt: MoreThanOrEqual(createdAfter) } : {}),
				...(status? { status: Like(`%${status}%`) } : {})
			},
			order : { travelsNodes : { order : "ASC"} }
		});
		return query;
	}

	async createTravel(vehicle: Number | null, nodes : any[] | null, user : string): Promise<Travels> {
		const vehicleId = await this.vehicleRepository.findOneBy({id: vehicle !== null ? Number(vehicle) : 0});
		const userEntity = await this.userRepository.findOneBy({username: user});
		const nodeList : TravelsNodes[] = [];
		if (vehicle !== null && !vehicleId) {
			throw new Error("Véhicule non trouvé");
		}
		for (const node of nodes !== null ? nodes : []) {
			const travelNode = await this.travelNodesRepository.findOneBy({id: node.id});
			if (!travelNode) {
				throw new Error(`Noeud de voyage non trouvé: ${node.id}`);
			}
			nodeList.push(travelNode);
		}
		nodeList.sort((a, b) => (a.order ?? 0) - (b.order ?? 0));
		const newTravel = new Travels({
			vehicleId: vehicle !== null ? Number(vehicle) : 0,
			createdAt: new Date(),
			status: "pending",
			estimatedDuration: null,
			orderedBy: userEntity?.username?? "",
			travelsNodes: nodeList
		});
		const savedTravel = await this.travelRepository.save(newTravel);
		return savedTravel;
		
	}

	async updateTravel(id: number, status: string , estimated_duration: number): Promise<Travels | null> {
		const travel = await this.travelRepository.findOneBy({ id });
		if (!travel) {
			return null;
		}
		travel.status = status;
		travel.estimatedDuration = estimated_duration;
		return this.travelRepository.save(travel);
		
	}
}

