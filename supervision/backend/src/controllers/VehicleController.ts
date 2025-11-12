import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetVehiclesRequest,
	CreateVehiclesRequest,
	UpdateVehicleRequest 
} from "../domain/requests/Vehicles";
import { VehicleService } from "../domain/services/VehicleService";

export class VehicleController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getVehicles(req: Request, res: Response) : Promise<void> {
		const request = GetVehiclesRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const vehicles = await vehicleService.getVehicles(request.id, request.name);
		res.status(HttpStatusEnum.OK).json(vehicles);
		return;
	}

	@Authenticated()
	public async createVehicle(req: Request, res: Response): Promise<void> {
		const request = CreateVehiclesRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const vehicle = await vehicleService.createVehicle(request.name!);
		res.status(HttpStatusEnum.OK).json(vehicle);
		return;
	}

	@Authenticated()
	public async updateVehicle(req: Request, res: Response): Promise<void> {
		const request = UpdateVehicleRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const vehicle = await vehicleService.updateVehicle(request.id, request.name)
		if (!vehicle) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Vehicle not found" });
		}
		res.status(HttpStatusEnum.OK).json(vehicle);
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

