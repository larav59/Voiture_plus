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


	static fromEntity(entity: Vehicles | null): VehiclesDTO | null {
		if (!entity) {
			return null;
		}
		const dto = new VehiclesDTO();
		dto.id = entity.id;
		dto.name = entity.name;
		// Need to map the first travel and state only with the concerned DTO classes return a null object if not present
		if (entity.travels && entity.travels.length > 0) dto.travel = entity.travels.map(v => TravelsDTO.fromEntity(v)) [0];
		if (entity.states && entity.states.length > 0) dto.state = entity.states.map(s => StatesDTO.fromEntity(s)) [0];
		if (!dto.travel) dto.travel = null;
		if (!dto.state) dto.state = null;
		return dto;
	}
}