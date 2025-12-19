import { Travels } from "../entities/Travels";
import { BaseEntityDTO } from "./BaseEntityDTO";
import { TravelsNodesDTO } from "./TravelsNodeDTO";
import { VehiclesDTO } from "./VehiclesDTO";

export class TravelsDTO extends Travels implements BaseEntityDTO<Travels> {
	id!: number
	createdAt!: Date | null;
	status!: string | null;
	estimatedDuration!: number | null;
	orderedBy!: string | null;
	nodes !: TravelsNodesDTO[] | null;
	vehicle !: VehiclesDTO;

	static fromEntity(entity: Travels | null): TravelsDTO | null {
		if (!entity) {
			return null;
		}
		const dto = new TravelsDTO();
		dto.id = entity.id;
		dto.createdAt = entity.createdAt;
		dto.status = entity.status;
		dto.estimatedDuration = entity.estimatedDuration;
		dto.orderedBy = entity.orderedBy;
		dto.vehicle = VehiclesDTO.fromEntity(entity.vehicle) !;
		dto.nodes = [];
		if (entity.travelsNodes && entity.travelsNodes.length > 0) {
			for (const travelNode of entity.travelsNodes) {
				const nodeDTO = TravelsNodesDTO.fromEntity(travelNode);
				if (nodeDTO) {
					dto.nodes.push(nodeDTO);
				}
			}
		}
		dto.nodes = dto.nodes.sort((a, b) => (a.order ?? 0) - (b.order ?? 0));
		return dto;
	}
}