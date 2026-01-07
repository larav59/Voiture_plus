import { Alarms } from "../entities/Alarms";
import { AlarmsTypes } from "../entities/AlarmsTypes";
import { Origins } from "../entities/Origins";
import { BaseEntityDTO } from "./BaseEntityDTO";

export class OriginsDTO extends Origins implements BaseEntityDTO<Origins> {
	id!: number
	label !: string | null;

	static fromEntity(entity: Origins | null): Origins | null {
		if (!entity) {
			return null;
		}
		const dto = new OriginsDTO();
		dto.id = entity.id;
		dto.label = entity.label;

		return dto;
	}
}