import { Nodes } from "../entities/Nodes";
import { NodesTypes } from "../entities/NodesTypes";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { NodesTypesDTO } from "./NodesTypesDTO";
import { StatesDTO } from "./StatesDTO";

export class NodesDTO extends Nodes implements BaseEntityDTO<Nodes> {
	id!: number
	name !: string | null
	description!: string | null
	positionX !: number | null
	positionY !: number | null
	offsetX !: number | null
	offsetY !: number | null
	nodeType !: NodesTypes

	static fromEntity(entity: Nodes | null): Nodes | null {
		if (!entity) {
			return null;
		}
		const dto = new NodesDTO();
		dto.id = entity.id;
		dto.name = entity.name;
		dto.description = entity.name;
		dto.positionX = entity.positionX;
		dto.positionY = entity.positionY;
		dto.offsetX = entity.offsetX;
		dto.offsetY = entity.offsetY;
		dto.nodeType = NodesTypesDTO.fromEntity(entity.nodeType) !;

		return dto;
	}
}