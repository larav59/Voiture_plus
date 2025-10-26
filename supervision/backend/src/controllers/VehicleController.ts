import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotImplemented } from "../infrastructure/webserver/errors/HttpError";
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
	public async getVehicules(req: Request, res: Response) : Promise<void> {
		const request = GetVehiclesRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw request.sendInvalidRequest(res,request);
		}
		const vehicles = await vehicleService.getVehicles(request.id, request.name);
		return res.status(HttpStatusEnum.OK).json(vehicles);
	}

	@Authenticated()
	public async createVehicle(req: Request, res: Response): Promise<void> {
		const request = CreateVehiclesRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw request.sendInvalidRequest(res, request);
		}

		const vehicle = await vehicleService.createVehicle(request.name!);
		return res.status(HttpStatusEnum.CREATED).json(vehicle);
	}

	@Authenticated()
	public async updateVehicle(req: Request, res: Response): Promise<void> {
		const request = UpdateVehicleRequest.fromRequest(req);
		const vehicleService = new VehicleService();

		if (request.validate().hasErrors()) {
			throw request.sendInvalidRequest(res, request);
		}

		const vehicle = await vehicleService.updateVehicle(request.id!, request.name!);
		if (!vehicle) {
			return res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Vehicle not found" });
		}
		return res.status(HttpStatusEnum.OK).json(vehicle);
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

