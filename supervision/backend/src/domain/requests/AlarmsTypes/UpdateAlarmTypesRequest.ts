import { query } from "express";
import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateAlarmTypesRequest extends BaseRequest<UpdateAlarmTypesRequest> {
	id : number;
	label : string;
	criticity : string;

	constructor(fields?: Partial<UpdateAlarmTypesRequest>) {
		super();
		this.id = fields?.id !;
		this.label = fields?.label !;
		this.criticity = fields?.criticity !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateAlarmTypesRequest> {
		const validator = new Validator<UpdateAlarmTypesRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("label").maxLength(255);
		validator.field("criticity").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateAlarmTypesRequest {
		const params = req.params;
		const body = req.body;
		return new UpdateAlarmTypesRequest({
			id: Number(params.id),
			label: body.label,
			criticity: body.criticity,
		});
	}
}