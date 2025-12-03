import { Nodes } from "../entities/Nodes";
import { NodesTypes } from "../entities/NodesTypes";
import { Travels } from "../entities/Travels";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { NodesDTO } from "./NodesDTO";

export class TravelsDTO extends Travels implements BaseEntityDTO<Travels> {
	id!: number
	createdAt!: Date | null;
	status!: string | null;
	estimatedDuration!: number | null;
	orderedBy!: string | null;
	nodes !: Nodes[] | null

	static fromEntity(entity: Travels | null): Travels | null {
		if (!entity) {
			return null;
		}
		const dto = new TravelsDTO();
		dto.id = entity.id;
		dto.createdAt = entity.createdAt;
		dto.status = entity.status;
		dto.estimatedDuration = entity.estimatedDuration;
		dto.orderedBy = entity.orderedBy;
		for (const tn of entity.travelsNodes ?? []) {
			if (tn.node) {
				if (!dto.nodes) {
					dto.nodes = [];
				}
				const nodeDTO = NodesDTO.fromEntity(tn.node) !;
				dto.nodes.push(nodeDTO);
			}
		}

		return dto;
	}
}