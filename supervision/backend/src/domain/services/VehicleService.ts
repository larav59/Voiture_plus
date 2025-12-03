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
			where: {
				...(id ? { id } : {}),
      			...(name ? { name: Like(`%${name}%`) } : {})
			},
			relations: {
				states: true,
				travels: {
					travelsNodes: {
						node : {
							nodeType : true
						}
					}
				},
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

			// find ongoing travel / active trip
			const ongoingTravel =
			v.travels?.find(t => t.status === "ongoing") 
			? [v.travels.find(t => t.status === "ongoing")!]
			: [];

			return {
			...v,
			states: latestState,
			travels: ongoingTravel,
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

