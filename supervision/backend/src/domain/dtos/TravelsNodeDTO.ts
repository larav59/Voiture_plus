import { TravelsNodes } from "../entities/TravelsNodes";	
import { Travels } from "../entities/Travels";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { NodesDTO } from "./NodesDTO";
import { Nodes } from "../entities/Nodes";

export class TravelsNodesDTO extends Travels implements BaseEntityDTO<TravelsNodes> {
	id!: number
	order !: number | null;
	arrivedAt !: Date | null;
	name !: string | null;
	pointOfInterest !: boolean | null;
	positionX !: number | null;
	positionY !: number | null;
	offsetX !: number | null;
	offsetY !: number | null
	


	static fromEntity(entity: TravelsNodes | null): TravelsNodesDTO | null {
		if (!entity) {
			return null;
		}
		const dto = new TravelsNodesDTO();
		dto.order = entity.order;
		dto.arrivedAt = entity.arrivedAt;
		const nodeDto = NodesDTO.fromEntity(entity.node) !;
		dto.id = nodeDto.id;
		dto.name = nodeDto.name;
		dto.pointOfInterest = nodeDto.pointOfInterest;
		dto.positionX = nodeDto.positionX;
		dto.positionY = nodeDto.positionY;
		dto.offsetX = nodeDto.offsetX;
		dto.offsetY = nodeDto.offsetY;
		return dto;
	}
}