import { Alarms } from "../entities/Alarms";
import { AlarmsTypes } from "../entities/AlarmsTypes";
import { Origins } from "../entities/Origins";
import { BaseEntityDTO } from "./BaseEntityDTO";

export class AlarmsDTO extends Alarms implements BaseEntityDTO<Alarms> {
	id!: number
	createdAt!: Date | null
	description!: string | null
	alarmType!: AlarmsTypes
	origin!: Origins

	static fromEntity(entity: Alarms | null): Alarms | null {
		if (!entity) {
			return null;
		}
		const dto = new AlarmsDTO();
		dto.id = entity.id;
		dto.createdAt = entity.createdAt;
		dto.description = entity.description;
		dto.alarmType = entity.alarmType;
		dto.origin = entity.origin;

		return dto;
	}
}