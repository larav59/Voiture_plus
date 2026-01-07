import { AlarmsTypes } from "../entities/AlarmsTypes";
import { BaseEntityDTO } from "./BaseEntityDTO";

export class AlarmTypesDTO extends AlarmsTypes implements BaseEntityDTO<AlarmsTypes> {
	id!: number
	label!: string | null
	criticity!: string | null

	static fromEntity(entity: AlarmsTypes | null): AlarmsTypes | null {
		if (!entity) {
			return null;
		}
		const dto = new AlarmTypesDTO();
		dto.id = entity.id;
		dto.label = entity.label;
		dto.criticity = entity.criticity;

		return dto;
	}
}