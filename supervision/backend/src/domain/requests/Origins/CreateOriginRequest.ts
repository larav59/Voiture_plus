import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateOriginRequest extends BaseRequest<CreateOriginRequest> {
	label : string;

	constructor(fields?: Partial<CreateOriginRequest>) {
		super();
		this.label = fields?.label !;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateOriginRequest> {
		const validator = new Validator<CreateOriginRequest>(this);
		validator.field("label").isRequired().maxLength(255);
		return validator;
	}

	static fromRequest(req: any): CreateOriginRequest {
		const body = req.body;
		return new CreateOriginRequest({
			label: body.label,
		});
	}
}