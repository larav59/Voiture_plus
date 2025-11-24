import { Repository, Like } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Origins } from "../entities/Origins";

export class OriginService {
	private originRepository: Repository<Origins>;

	constructor() {
		this.originRepository = AppDataSource.getRepository(Origins);
	}

	// Méthode pour récupérer les origines
	async getOrigins(label: string | null): Promise<Origins[]> {
		const query = this.originRepository.find({
			where: {
				...(label !== null ? { label: Like(`%${label}%`) } : {})
			}
		});
		return query;
	}

	async createOrigin(label: string): Promise<Origins> {
		const newOrigin = this.originRepository.create({ label });
		return this.originRepository.save(newOrigin);
	}

	async updateOrigin(id: number, label: string): Promise<Origins | null> {
		const origin = await this.originRepository.findOneBy({ id });
		if (!origin) {
			return null;
		}
		origin.label = label;
		return this.originRepository.save(origin);
	}
}

