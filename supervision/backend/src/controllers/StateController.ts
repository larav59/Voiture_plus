import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetStateRequest,
	CreateStateRequest,
	UpdateStateRequest
} from "../domain/requests/States";
import { TravelService } from "../domain/services/TravelService";
import { TravelsDTO } from "../domain/dtos/TravelsDTO";
import { StateService } from "../domain/services";
import { StatesDTO } from "../domain/dtos/StatesDTO";

export class StateController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getStates(req: Request, res: Response) : Promise<void> {
		const request = GetStateRequest.fromRequest(req);
		const stateService = new StateService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const states = await stateService.getStates(request.vehicleId ?? 0, request.occuredBefore ?? "", request.occuredAfter ?? "");
		const statesDTO = states.map(s => StatesDTO.fromEntity(s));
		res.status(HttpStatusEnum.OK).json(statesDTO);
		return;
	}

	@Authenticated()
	public async createState(req: Request, res: Response): Promise<void> {
		const request = CreateStateRequest.fromRequest(req);
		const stateService = new StateService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const state = await stateService.createState(
			request.vehicleId ?? 0, 
			request.speed ?? 0, 
			request.angle ?? 0, 
			request.positionX ?? 0, 
			request.positionY ?? 0,
			request.occuredAt ?? new Date()
		);
		const stateDTO = StatesDTO.fromEntity(state) !;
		res.status(HttpStatusEnum.OK).json(stateDTO);
		return;
	}

	@Authenticated()
	public async updateState(req: Request, res: Response): Promise<void> {
		const request = UpdateStateRequest.fromRequest(req);
		const stateService = new StateService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const state = await stateService.updateState(
			request.vehicleId ?? 0, 
			request.speed ?? 0, 
			request.angle ?? 0, 
			request.positionX ?? 0, 
			request.positionY ?? 0, 
			request.occuredAt ?? new Date()
		);
		if (!state) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "State not found" });
		}
		const stateDTO = StatesDTO.fromEntity(state) !;
		res.status(HttpStatusEnum.OK).json(stateDTO);
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

