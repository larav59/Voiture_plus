import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateAlarmRequest extends BaseRequest<CreateAlarmRequest> {
	description : string | null;
	type : number;
	origin : number;

	constructor(fields?: Partial<CreateAlarmRequest>) {
		super();
		this.description = fields?.description!;
		this.type = fields?.type!;
		this.origin = fields?.origin!;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateAlarmRequest> {
		const validator = new Validator<CreateAlarmRequest>(this);
		validator.field("type").isRequired().isNumber().greaterThan(0);
		validator.field("origin").isRequired().isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): CreateAlarmRequest {
		const body = req.body;
		return new CreateAlarmRequest({
			description: body.description,
			type: Number(body.type),
			origin: Number(body.origin),
		});
	}
}