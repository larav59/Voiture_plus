import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateAlarmRequest extends BaseRequest<UpdateAlarmRequest> {
	id : number;
	description : string;
	type : number;
	origin : number;

	constructor(fields?: Partial<UpdateAlarmRequest>) {
		super();
		this.id = fields?.id !;
		this.description = fields?.description !;
		this.type = fields?.type !;
		this.origin = fields?.origin !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateAlarmRequest> {
		const validator = new Validator<UpdateAlarmRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("type").isNumber().greaterThan(0);
		validator.field("origin").isNumber().greaterThan(0);
		validator.field("description").maxLength(255);
		return validator;
	}

	static fromRequest(req: any): UpdateAlarmRequest {
		const params = req.params;
		const body = req.body;
		return new UpdateAlarmRequest({
			id: Number(params.id),
			description: body.description,
			type: Number(body.type),
			origin: Number(body.origin)
		});
	}
}