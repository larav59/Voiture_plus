import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateAlarmRequest extends BaseRequest<UpdateAlarmRequest> {
	id : number;
	weight : number | null;
	origineId : number | null;
	destinationId : number | null;

	constructor(fields?: Partial<UpdateAlarmRequest>) {
		super();
		this.id = fields?.id ?? 0;
		this.weight = fields?.weight ?? null;
		this.origineId = fields?.origineId ?? 0;
		this.destinationId = fields?.destinationId ?? 0;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateAlarmRequest> {
		const validator = new Validator<UpdateAlarmRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("origineId").isNumber().greaterThan(0);
		validator.field("destinationId").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateAlarmRequest {
		return new UpdateAlarmRequest({
			id: req.params.id,
			weight: req.body.weight,
			origineId: req.body.origineId,
			destinationId: req.body.destinationId
		});
	}
}