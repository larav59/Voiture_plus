import { Repository, MoreThan, LessThan } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { States } from "../entities/States";
import { Vehicles } from "../entities/Vehicles";

export class StateService {
    private StatesRepository: Repository<States>;
    private vehicleRepository: Repository<Vehicles>;

    constructor() {
        this.StatesRepository = AppDataSource.getRepository(States);
        this.vehicleRepository = AppDataSource.getRepository(Vehicles);
    }

    // Méthode pour récupérer les origines
    async getStates(vehicle: number | null,occuredBefore: Date | "" , occuredAfter: Date | ""): Promise<States[]> {
        const query = this.StatesRepository.find({
            relations: ["vehicle"],
            where: {
                ...(vehicle ? { vehicleId: vehicle } : {}),
				...(occuredBefore ? { occuredAt: LessThan(occuredBefore) } : {}),
				...(occuredAfter? { occuredAt: MoreThan(occuredAfter) } : {})
			}
        });
        return query;
    }

    async createState(vehicle : number, speed : number, angle : number , positionX: number, positionY: number, occuredAt: Date): Promise<States> {
        const vehicleRef = await this.vehicleRepository.findOneBy({id: vehicle});
        if (!vehicleRef) {
            throw new Error("Véhicule non trouvé");
        }
        if (positionX === undefined || positionY === undefined) {
            throw new Error("Position invalide");
        }
		
		if (speed === undefined || angle === undefined) {
			throw new Error("Données de l'état invalides");
		}
        const newState = this.StatesRepository.create({
			vehicleId: vehicle,
			occuredAt: occuredAt,
			speed: speed,
			angle: angle,
			positionX: positionX,
			positionY: positionY
		});
        return this.StatesRepository.save(newState);
        
    }

    async updateState(vehicle : number, speed : number, angle : number , positionX: number, positionY: number, occuredAt: Date): Promise<States> {
        const state = await this.StatesRepository.findOneBy({vehicleId: vehicle});
		if (!state) {
			throw new Error("État non trouvé");
		}
		if (positionX !== undefined)  state.positionX = positionX;
		if (positionY !== undefined)  state.positionY = positionY;
		if (speed !== undefined) state.speed = speed;
		if (angle !== undefined) state.angle = angle;
        if (occuredAt !== undefined) state.occuredAt = occuredAt;

		return this.StatesRepository.save(state);
        
    }
}


