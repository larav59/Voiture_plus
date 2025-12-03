import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateTravelRequest extends BaseRequest<UpdateTravelRequest> {
	duration : number | null;
	status : string | null;
	id : number | null;

	constructor(fields?: Partial<UpdateTravelRequest>) {
		super();
		this.duration = fields?.duration ?? null;
		this.status = fields?.status ?? null;
		this.id = fields?.id ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateTravelRequest> {
		const validator = new Validator<UpdateTravelRequest>(this);
		validator.field("id").isNumber().isRequired();
		validator.field("duration").isNumber();
		return validator;
	}

	static fromRequest(req: any): UpdateTravelRequest {
		const body = req.body
		const params = req.params;
		return new UpdateTravelRequest({
			id: params.id,
			duration: body.duration,
			status: body.status
		});
	}
}