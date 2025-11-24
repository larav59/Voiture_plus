import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	CreateOriginRequest,
	GetOriginRequest,
	UpdateOriginRequest
} from "../domain/requests/Origins";
import { OriginService } from "../domain/services/OriginService";

export class OriginController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getOrigins(req: Request, res: Response) : Promise<void> {
		const request = GetOriginRequest.fromRequest(req);
		const originService = new OriginService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const origins = await originService.getOrigins(request.label ?? "");
		res.status(HttpStatusEnum.OK).json(origins);
		return;
	}

	@Authenticated()
	public async createOrigin(req: Request, res: Response): Promise<void> {
		const request = CreateOriginRequest.fromRequest(req);
		const originService = new OriginService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const origin = await originService.createOrigin(request.label ?? "");
		res.status(HttpStatusEnum.OK).json(origin);
		return;
	}

	@Authenticated()
	public async updateOrigin(req: Request, res: Response): Promise<void> {
		const request = UpdateOriginRequest.fromRequest(req);
		const originService = new OriginService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const origin = await originService.updateOrigin(request.id, request.label ?? "");
		if (!origin) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Origin not found" });
		}
		res.status(HttpStatusEnum.OK).json(origin);
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

