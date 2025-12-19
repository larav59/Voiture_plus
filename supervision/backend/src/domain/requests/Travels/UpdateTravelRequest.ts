import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateTravelRequest extends BaseRequest<UpdateTravelRequest> {
	duration : number;
	status : string;
	id : number;

	constructor(fields?: Partial<UpdateTravelRequest>) {
		super();
		this.duration = fields?.duration !;
		this.status = fields?.status !;
		this.id = fields?.id !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateTravelRequest> {
		const validator = new Validator<UpdateTravelRequest>(this);
		validator.field("id").isNumber().isRequired().greaterThan(0);
		validator.field("duration").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateTravelRequest {
		const body = req.body
		const params = req.params;
		return new UpdateTravelRequest({
			id: Number(params.id),
			duration: body.duration,
			status: body.status
		});
	}
}