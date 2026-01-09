import { Nodes } from "../entities/Nodes";
import { NodesTypes } from "../entities/NodesTypes";
import { BaseEntityDTO } from "./BaseEntityDTO";

export class NodesTypesDTO extends NodesTypes implements BaseEntityDTO<NodesTypes> {
	id!: number
	label !: string | null

	static fromEntity(entity: NodesTypesDTO | null): NodesTypes | null {
		if (!entity) {
			return null;
		}
		const dto = new NodesTypesDTO();
		dto.id = entity.id;
		dto.label = entity.label;
		return dto;
	}
}