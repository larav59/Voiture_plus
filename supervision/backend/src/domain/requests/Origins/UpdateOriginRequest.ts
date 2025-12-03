import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateOriginRequest extends BaseRequest<UpdateOriginRequest> {
	label : string | null;
	id : number | null;

	constructor(fields?: Partial<UpdateOriginRequest>) {
		super();
		this.label = fields?.label ?? null;
		this.id = fields?.id ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateOriginRequest> {
		const validator = new Validator<UpdateOriginRequest>(this);
		validator.field("label").maxLength(100);
		validator.field("id").isRequired();
		return validator;
	}

	static fromRequest(req: any): UpdateOriginRequest {
		const body = req.body
		const params = req.params;
		return new UpdateOriginRequest({
			id: params.id,
			label: body.label,
		});
	}
}