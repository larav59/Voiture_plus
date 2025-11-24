import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateAlarmRequest extends BaseRequest<UpdateAlarmRequest> {
	id : number;
	description : string;
	type : number;
	origin : number;

	constructor(fields?: Partial<UpdateAlarmRequest>) {
		super();
		this.id = fields?.id ?? 0;
		this.description = fields?.description ?? "";
		this.type = fields?.type ?? 0;
		this.origin = fields?.origin ?? 0;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateAlarmRequest> {
		const validator = new Validator<UpdateAlarmRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("type").isNumber().greaterThan(0);
		validator.field("origin").isRequired().isNumber().greaterThan(0);
		validator.field("description").isRequired().maxLength(255);
		return validator;
	}

	static fromRequest(req: any): UpdateAlarmRequest {
		const body = req.body
		const params = req.params;
		params.id = params.id !== undefined ? parseInt(params.id) : undefined;
		body.description = body.description !== undefined ? String(body.description) : undefined;
		body.type = body.type !== undefined ? parseInt(body.type) : undefined;
		body.origin = body.origin !== undefined ? parseInt(body.origin) : undefined;

		return new UpdateAlarmRequest({
			id: params.id,
			description: body.description,
			type: body.type,
			origin: body.origin
		});
	}
}