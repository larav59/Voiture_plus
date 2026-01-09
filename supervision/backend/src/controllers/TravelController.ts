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
import { TravelsDTO } from "../domain/dtos/TravelsDTO";
import { MqttClientService } from "../domain/services/MqttClientService";

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
		const travels = await travelService.getTravels(
			request.vehicleId ?? 0,
			request.createdBefore ?? "",
			request.createdAfter ?? "", 
			request.status
		);
		const travelsDTO = travels.map(t => TravelsDTO.fromEntity(t));
		res.status(HttpStatusEnum.OK).json(travelsDTO);
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
		const travelDTO = TravelsDTO.fromEntity(travel) !;
		res.status(HttpStatusEnum.OK).json(travelDTO);

		const nodeList : any[] = [];
		travel.travelsNodes.forEach(node => {
			nodeList.push(node.node.id);
		});

		const mqttClientService = new MqttClientService();
		const MqttRequest = {
			commandId: 'REQ_PLAN_' + new Date().getTime(),
			action : `PLAN_ROUTE_REQUEST`,
			carId : travel.vehicleId,
			nodeList : nodeList,
			replyTopic : `services/api/response`
		} 
		mqttClientService.publish('services/route-planner/request', JSON.stringify(MqttRequest));
		mqttClientService.addToQueue(MqttRequest);
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
		const travelDTO = TravelsDTO.fromEntity(travel) !;
		res.status(HttpStatusEnum.OK).json(travelDTO);
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

