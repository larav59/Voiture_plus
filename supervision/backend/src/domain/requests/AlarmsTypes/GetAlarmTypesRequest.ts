import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetAlarmTypesRequest extends BaseRequest<GetAlarmTypesRequest> {
	label : string | null;
	criticity : string | null;

	constructor(fields?: Partial<GetAlarmTypesRequest>) {
		super();
		this.label = fields?.label ?? null;
		this.criticity = fields?.criticity ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetAlarmTypesRequest> {
		const validator = new Validator<GetAlarmTypesRequest>(this);
		validator.field("label").maxLength(255);
		validator.field("criticity").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetAlarmTypesRequest {
		const query = req.query;
		return new GetAlarmTypesRequest({
			label: query.label ?? null,
			criticity: query.criticity ?? null,
		});
	}
}