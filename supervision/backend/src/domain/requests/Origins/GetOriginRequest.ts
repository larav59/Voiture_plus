import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetOriginRequest extends BaseRequest<GetOriginRequest> {
	label : string | "";

	constructor(fields?: Partial<GetOriginRequest>) {
		super();
		this.label = fields?.label ?? "";
	}

	// Méthode pour la validation
	public validate(): Validator<GetOriginRequest> {
		const validator = new Validator<GetOriginRequest>(this);
		validator.field("label").maxLength(100);
		return validator;
	}

	static fromRequest(req: any): GetOriginRequest {
		const queryString = req.query;
		queryString.label = queryString.label !== undefined ? String(queryString.label) : undefined;

		return new GetOriginRequest({
			label: queryString.label,
		});
	}
}