import { Nodes } from "../entities/Nodes";
import { States } from "../entities/States";
import { BaseEntityDTO } from "./BaseEntityDTO";

export class StatesDTO extends States implements BaseEntityDTO<States> {
	id!: number
	speed!: number | null;
	angle!: number | null;
	positionX !: number | null
	positionY !: number | null
	occuredAt !: Date | null

	static fromEntity(entity: States | null): States | null {
		if (!entity) {
			return null;
		}
		const dto = new StatesDTO();
		dto.id = entity.id;
		dto.speed = entity.speed;
		dto.angle = entity.angle;
		dto.positionX = entity.positionX;
		dto.positionY = entity.positionY;
		dto.occuredAt = entity.occuredAt;
		return dto;
	}
}