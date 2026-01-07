import { query } from "express";
import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateStateRequest extends BaseRequest<CreateStateRequest> {
	public vehicleId: number | null;
	public speed: number | null;
	public angle: number | null;
	public positionX: number | null;
	public positionY: number | null;
	public occuredAt: Date | null;

	constructor(fields?: Partial<CreateStateRequest>) {
		super();
		this.vehicleId = fields?.vehicleId ?? null;
		this.speed = fields?.speed ?? null;
		this.angle = fields?.angle ?? null;
		this.positionX = fields?.positionX ?? null;
		this.positionY = fields?.positionY ?? null;
		this.occuredAt = fields?.occuredAt ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateStateRequest> {
		const validator = new Validator<CreateStateRequest>(this);
		validator.field("vehicleId").isRequired().isNumber().greaterThan(0);
		validator.field("speed").isRequired().isNumber();
		validator.field("angle").isRequired().isNumber();
		validator.field("positionX").isRequired().isNumber();
		validator.field("positionY").isRequired().isNumber();
		//validator.field("occuredAt").isRequired().isDate();	
		return validator;
	}

	static fromRequest(req: any): CreateStateRequest {
		const body = req.body;
		const params = req.params;
		return new CreateStateRequest({
			vehicleId: params.vehicleId ? Number(params.vehicleId): null,
			speed: body.speed,
			angle: body.angle,
			positionX: body.positionX,
			positionY: body.positionY,
			occuredAt: body.occuredAt ? new Date(body.occuredAt) : new Date(),
		});
	}
}