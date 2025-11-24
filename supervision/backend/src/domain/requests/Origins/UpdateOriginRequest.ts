import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateOriginRequest extends BaseRequest<UpdateOriginRequest> {
	label : string | "";
	id : number | 0;

	constructor(fields?: Partial<UpdateOriginRequest>) {
		super();
		this.label = fields?.label ?? "";
		this.id = fields?.id ?? 0;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateOriginRequest> {
		const validator = new Validator<UpdateOriginRequest>(this);
		validator.field("label").isRequired();
		validator.field("id").isRequired();
		return validator;
	}

	static fromRequest(req: any): UpdateOriginRequest {
		const body = req.body
		const params = req.params;
		params.id = params.id !== undefined ? parseInt(params.id) : undefined;
		body.label = body.label !== undefined ? String(body.label) : undefined;
		
		return new UpdateOriginRequest({
			id: params.id,
			label: body.label,
		});
	}
}