import { Repository, Like } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Vehicles } from "../entities/Vehicles";

export class VehicleService {
	private vehicleRepository: Repository<Vehicles>;

	constructor() {
		this.vehicleRepository = AppDataSource.getRepository(Vehicles);
	}

	// Méthode pour récupérer les véhicules
	async getVehicles(id: number | null, name: string | null): Promise<Vehicles[]> {
		const query = this.vehicleRepository.find({
			where: {
				...(id !== null ? { id } : {}),
				...(name !== null ? { name: Like(`%${name}%`) } : {}),
			},
		});
		return query;
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

