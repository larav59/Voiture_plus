import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetAlarmTypesRequest,
	CreateAlarmTypesRequest,
	UpdateAlarmTypesRequest
} from "../domain/requests/AlarmsTypes";
import { AlarmTypesService } from "../domain/services/AlarmTypesService";
import { AlarmTypesDTO } from "../domain/dtos/AlarmsTypesDTO";

export class AlarmTypesController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getAlarmTypes(req: Request, res: Response) : Promise<void> {
		const request = GetAlarmTypesRequest.fromRequest(req);
		const alarmsTypesService = new AlarmTypesService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const alarms = await alarmsTypesService.getAlarmTypes(
			request.label, 
			request.criticity
		);
		res.status(HttpStatusEnum.OK).json(alarms);
		return;
	}

	@Authenticated()
	public async createAlarmTypes(req: Request, res: Response): Promise<void> {
		const request = CreateAlarmTypesRequest.fromRequest(req);
		const alarmTypesService = new AlarmTypesService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const alarmTypes = await alarmTypesService.createAlarmTypes(
			request.label ?? "", 
			request.criticity!

		);
	const alarmTypesDTO = AlarmTypesDTO.fromEntity(alarmTypes);
		res.status(HttpStatusEnum.OK).json(alarmTypesDTO);
		return;
	}

	@Authenticated()
	public async updateAlarmTypes(req: Request, res: Response): Promise<void> {
		const request = UpdateAlarmTypesRequest.fromRequest(req);
		const alarmTypesService = new AlarmTypesService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const alarm = await alarmTypesService.updateAlarmTypes(
			request.id, 
			request.label,
			request.criticity
		);
		if (!alarm) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Alarm not found" });
		}
		const alarmTypesDTO = AlarmTypesDTO.fromEntity(alarm);
		res.status(HttpStatusEnum.OK).json(alarmTypesDTO);
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

