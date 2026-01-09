import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetNodeTypeRequest,
	CreateNodeTypeRequest,
	UpdateNodeTypeRequest
} from "../domain/requests/NodeTypes";
import { NodeTypeService } from "../domain/services/NodeTypeService";
import { NodesTypesDTO } from "../domain/dtos/NodesTypesDTO";

export class NodeTypeController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getNodeTypes(req: Request, res: Response) : Promise<void> {
		const request = GetNodeTypeRequest.fromRequest(req);
		const nodeTypeService = new NodeTypeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const nodeTypes = await nodeTypeService.getNodeTypes(request.label ?? "");
		res.status(HttpStatusEnum.OK).json(nodeTypes);
		return;
	}

	@Authenticated()
	public async createNodeType(req: Request, res: Response): Promise<void> {
		const request = CreateNodeTypeRequest.fromRequest(req);
		const nodeTypeService = new NodeTypeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const nodeType = await nodeTypeService.createNodeType(request.label!);
		const nodeTypeDTO = NodesTypesDTO.fromEntity(nodeType) !;
		res.status(HttpStatusEnum.OK).json(nodeTypeDTO);
		return;
	}

	@Authenticated()
	public async updateNodeType(req: Request, res: Response): Promise<void> {
		const request = UpdateNodeTypeRequest.fromRequest(req);
		const nodeTypeService = new NodeTypeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const nodeType = await nodeTypeService.updateNodeType(request.id, request.label !);
		if (!nodeType) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "NodeType not found" });
		}
		const nodeTypeDTO = NodesTypesDTO.fromEntity(nodeType) !;
		res.status(HttpStatusEnum.OK).json(nodeTypeDTO);
		return ;
	}

	@Authenticated()
	async echo(req: Request, res: Response) {
		res.json({
			identity: req.identity,
			body: req.body
		});
	}

	async status(req: Request, res: Response) {
		throw new NotImplemented("Status endpoint not implemented yet");
	}
}

