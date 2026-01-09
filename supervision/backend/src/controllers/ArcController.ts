import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetArcRequest,
	CreateArcRequest,
	UpdateArcRequest
} from "../domain/requests/Arcs";
import { ArcsDTO } from "../domain/dtos/ArcsDTO";
import { ArcService } from "../domain/services/ArcService";

export class ArcController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getArcs(req: Request, res: Response) : Promise<void> {
		const request = GetArcRequest.fromRequest(req);
		const arcsService = new ArcService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const arcs = await arcsService.getArcs(request.originNode ?? 0, request.destinationNode ?? 0, request.type ?? null);
		const arcDTO = arcs.map(arc => ArcsDTO.fromEntity(arc));
		res.status(HttpStatusEnum.OK).json(arcDTO);
		return;
	}

	@Authenticated()
	public async createArc(req: Request, res: Response): Promise<void> {
		const request = CreateArcRequest.fromRequest(req);
		const arcService = new ArcService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const arc = await arcService.createArc(request.weight!, request.originNode!, request.destinationNode!, request.type!);
		const arcDTO = ArcsDTO.fromEntity(arc);
		res.status(HttpStatusEnum.OK).json(arcDTO);
		return;
	}

	@Authenticated()
	public async updateArc(req: Request, res: Response): Promise<void> {
		const request = UpdateArcRequest.fromRequest(req);
		const arcService = new ArcService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const arc = await arcService.updateArc(request.id, request.weight!, request.originNode!, request.destinationNode!, request.type!);
		if (!arc) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Arc not found" });
		}
		const arcDTO = ArcsDTO.fromEntity(arc);
		res.status(HttpStatusEnum.OK).json(arcDTO);
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

