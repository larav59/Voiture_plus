import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetTravelRequest,
	CreateTravelRequest,
	UpdateTravelRequest
} from "../domain/requests/Travels";
import { TravelService } from "../domain/services/TravelService";

export class TravelController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getTravels(req: Request, res: Response) : Promise<void> {
		const request = GetTravelRequest.fromRequest(req);
		const travelService = new TravelService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const travels = await travelService.getTravels(request.vehicle ?? 0, request.createdBefore ?? "", request.createdAfter ?? "", request.status);
		res.status(HttpStatusEnum.OK).json(travels);
		return;
	}

	@Authenticated()
	public async createTravel(req: Request, res: Response): Promise<void> {
		const request = CreateTravelRequest.fromRequest(req);
		const travelService = new TravelService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const travel = await travelService.createTravel(request.vehicle, request.nodes, req.identity.username);
		res.status(HttpStatusEnum.OK).json(travel);
		return;
	}

	@Authenticated()
	public async updateTravel(req: Request, res: Response): Promise<void> {
		const request = UpdateTravelRequest.fromRequest(req);
		const travelService = new TravelService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const travel = await travelService.updateTravel(request.id ?? 0, request.status ?? "", request.duration ?? 0);
		if (!travel) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Travel not found" });
		}
		res.status(HttpStatusEnum.OK).json(travel);
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

