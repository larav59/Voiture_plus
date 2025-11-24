import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateOriginRequest extends BaseRequest<CreateOriginRequest> {
	label : string | null;

	constructor(fields?: Partial<CreateOriginRequest>) {
		super();
		this.label = fields?.label ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateOriginRequest> {
		const validator = new Validator<CreateOriginRequest>(this);
		validator.field("label").isRequired().maxLength(100);
		return validator;
	}

	static fromRequest(req: any): CreateOriginRequest {
		const body = req.body
		body.label = body.label !== undefined ? String(body.label) : undefined;

		return new CreateOriginRequest({
			label: body.label,
		});
	}
}