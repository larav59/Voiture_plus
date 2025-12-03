import { Vehicles } from "../entities/Vehicles";
import { Travels } from "../entities/Travels";
import { States } from "../entities/States";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { TravelsDTO } from "./TravelsDTO";
import { StatesDTO } from "./StatesDTO";

export class VehiclesDTO extends Vehicles implements BaseEntityDTO<Vehicles> {
	id!: number
	name!: string | null
	travel !: Travels | null
	state !: States | null


	static fromEntity(entity: Vehicles | null): Vehicles | null {
		if (!entity) {
			return null;
		}
		const dto = new VehiclesDTO();
		dto.id = entity.id;
		dto.name = entity.name;
		// Need to map the first travel and state only with the concerned DTO classes
		dto.travel = entity.travels.map(v => TravelsDTO.fromEntity(v)) [0] ?? null;
		dto.state = entity.states.map(s => StatesDTO.fromEntity(s)) [0] ?? null;

		return dto;
	}
}