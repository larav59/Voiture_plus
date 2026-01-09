import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetOriginRequest extends BaseRequest<GetOriginRequest> {
	label : string | null;

	constructor(fields?: Partial<GetOriginRequest>) {
		super();
		this.label = fields?.label ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetOriginRequest> {
		const validator = new Validator<GetOriginRequest>(this);
		validator.field("label").maxLength(255);
		return validator;
	}

	static fromRequest(req: any): GetOriginRequest {
		const query = req.query;
		return new GetOriginRequest({
			label: query.label,
		});
	}
}