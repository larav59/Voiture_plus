import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetNodeRequest,
	CreateNodeRequest,
	UpdateNodeRequest
} from "../domain/requests/Nodes";
import { NodeService } from "../domain/services/NodeService";
import { NodesDTO } from "../domain/dtos/NodesDTO";

export class NodeController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getNodes(req: Request, res: Response) : Promise<void> {
		const request = GetNodeRequest.fromRequest(req);
		const nodeService = new NodeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const nodes = await nodeService.getNodes(request.isPointOfInterest !);
		const nodesDTO = nodes.map(n => NodesDTO.fromEntity(n));
		res.status(HttpStatusEnum.OK).json(nodesDTO);
		return;
	}

	@Authenticated()
	public async createNode(req: Request, res: Response): Promise<void> {
		const request = CreateNodeRequest.fromRequest(req);
		const nodeService = new NodeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const node = await nodeService.createNode(
			request.name,
			request.positionX,
			request.positionY,
			request.offsetX,
			request.offsetY,
			request.isPointOfInterest,
			request.type
		);
		const nodeDTO = NodesDTO.fromEntity(node) !;
		res.status(HttpStatusEnum.OK).json(nodeDTO);
		return;
	}

	@Authenticated()
	public async updateNode(req: Request, res: Response): Promise<void> {
		const request = UpdateNodeRequest.fromRequest(req);
		const nodeService = new NodeService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const position = {
			"positionX" : request.positionX,
			"positionY" : request.positionY,
			"offsetX" : request.offsetX,
			"offsetY" : request.offsetY,
		}

		const node = await nodeService.updateNode(request.id, request.name!, position, request.isPointOfInterest!, request.type!);
		if (!node) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Node not found" });
		}
		const nodeDTO = NodesDTO.fromEntity(node) !;
		res.status(HttpStatusEnum.OK).json(nodeDTO);
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

