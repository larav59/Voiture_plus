import { Arcs } from "../entities/Arcs";
import { Nodes } from "../entities/Nodes";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { NodesDTO } from "./NodesDTO";

export class ArcsDTO extends Arcs implements BaseEntityDTO<Arcs> {
	id!: number
	originNodeId !: number | null
	destinationNodeId !: number | null
	weight!: number | null

	static fromEntity(entity: Arcs  | null): ArcsDTO | null {
		if (!entity) {
			return null;
		}
		const dto = new ArcsDTO();
		dto.id = entity.id;
		dto.originNodeId = entity.originNodeId ;
		dto.destinationNodeId = entity.destinationNodeId;
		dto.weight = entity.weight;
		
		return dto;
	}
}