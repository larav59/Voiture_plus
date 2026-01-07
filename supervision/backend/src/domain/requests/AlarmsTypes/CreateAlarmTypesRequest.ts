import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateAlarmTypesRequest extends BaseRequest<CreateAlarmTypesRequest> {
	label : string;
	criticity : string;

	constructor(fields?: Partial<CreateAlarmTypesRequest>) {
		super();
		this.label = fields?.label!;
		this.criticity = fields?.criticity!;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateAlarmTypesRequest> {
		const validator = new Validator<CreateAlarmTypesRequest>(this);
		validator.field("label").isRequired().maxLength(255);
		validator.field("criticity").isRequired().isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): CreateAlarmTypesRequest {
		const body = req.body;
		return new CreateAlarmTypesRequest({
			label: body.label!,
			criticity: body.criticity!,
		});
	}
}