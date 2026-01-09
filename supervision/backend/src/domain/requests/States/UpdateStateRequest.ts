import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateStateRequest extends BaseRequest<UpdateStateRequest> {
	public vehicleId : number 
	public stateId: number 
	public speed: number 
	public angle: number 
	public positionX: number 
	public positionY: number 
	public occuredAt: Date 

	constructor(fields?: Partial<UpdateStateRequest>) {
		super();
		this.vehicleId = fields?.vehicleId !;
		this.stateId = fields?.stateId !;
		this.speed = fields?.speed !;
		this.angle = fields?.angle !;
		this.positionX = fields?.positionX !;
		this.positionY = fields?.positionY !;
		this.occuredAt = fields?.occuredAt !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateStateRequest> {
		const validator = new Validator<UpdateStateRequest>(this);
		validator.field("vehicleId").isRequired().isNumber().greaterThan(0);
		validator.field("stateId").isRequired().isNumber().greaterThan(0);
		validator.field("speed").isNumber();
		validator.field("angle").isNumber();
		validator.field("positionX").isNumber();
		validator.field("positionY").isNumber();
		validator.field("occuredAt").isDate();
		return validator;
	}

	static fromRequest(req: any): UpdateStateRequest {
		const body = req.body
		const params = req.params;
		return new UpdateStateRequest({
			vehicleId: Number(params.vehicleId),
			stateId: Number(params.stateId),
			speed: body.speed,
			angle: body.angle,
			positionX: body.positionX,
			positionY: body.positionY,
			occuredAt: new Date(body.occuredAt)
		});
	}
}