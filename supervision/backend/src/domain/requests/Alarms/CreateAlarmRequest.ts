import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateAlarmRequest extends BaseRequest<CreateAlarmRequest> {
	description : string | null;
	type : number | null;
	origin : number | null;

	constructor(fields?: Partial<CreateAlarmRequest>) {
		super();
		this.description = fields?.description ?? null;
		this.type = fields?.type ?? null;
		this.origin = fields?.origin ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateAlarmRequest> {
		const validator = new Validator<CreateAlarmRequest>(this);
		validator.field("type").isNumber().greaterThan(0);
		validator.field("origin").isRequired().isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): CreateAlarmRequest {
		const body = req.body
		body.description = body.description !== undefined ? String(body.description) : undefined;
		body.type = body.type !== undefined ? parseInt(body.type) : undefined;
		body.origin = body.origin !== undefined ? parseInt(body.origin) : undefined;

		return new CreateAlarmRequest({
			description: body.description,
			type: body.type,
			origin: body.origin,
		});
	}
}