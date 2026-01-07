import { Repository, Like , } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Vehicles } from "../entities/Vehicles";

export class VehicleService {
	private vehicleRepository: Repository<Vehicles>;

	constructor() {
		this.vehicleRepository = AppDataSource.getRepository(Vehicles);
	}

	// Méthode pour récupérer les véhicules
	async getVehicles(id?: number, name?: string): Promise<Vehicles[]> {

		const vehicles = await this.vehicleRepository.find({
			relations : ["states", "travels", "travels.travelsNodes", "travels.travelsNodes.node", "travels.travelsNodes.node.nodeType"],
			where: {
				...(id ? { id } : {}),
      			...(name ? { name: Like(`%${name}%`) } : {})
			},
			relationLoadStrategy: "query",
			order: {
				states: { occuredAt: "DESC" }, 
				travels: { createdAt: "DESC" },
			}
		});
		
		return vehicles.map(v => {

			// keep ONLY the most recent state
			const latestState = v.states?.[0] ? [v.states[0]] : [];
			const latestTravel = v.travels?.[0] ? [v.travels[0]] : [];
			return {
			...v,
			states: latestState,
			travels: latestTravel,
			};
		});
	}


	async createVehicle(name: string): Promise<Vehicles> {
		const newVehicle = this.vehicleRepository.create({ name });
		return this.vehicleRepository.save(newVehicle);
	}

	async updateVehicle(id: number, name: string): Promise<Vehicles | null> {
		const vehicle = await this.vehicleRepository.findOneBy({ id });
		if (!vehicle) {
			return null;
		}
		vehicle.name = name;
		return this.vehicleRepository.save(vehicle);
	}
}

