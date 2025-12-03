import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetTravelRequest extends BaseRequest<GetTravelRequest> {
	vehicle : number | null;
	createdAfter : Date | null;
	createdBefore : Date | null;
	status : string | null;

	constructor(fields?: Partial<GetTravelRequest>) {
		super();
		this.vehicle = fields?.vehicle ?? null;
		this.createdAfter = fields?.createdAfter ?? null;
		this.createdBefore = fields?.createdBefore ?? null;
		this.status = fields?.status ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetTravelRequest> {
		const validator = new Validator<GetTravelRequest>(this);
		validator.field("vehicle").isNumber();
		return validator;
	}

	static fromRequest(req: any): GetTravelRequest {
		const query = req.query;
		return new GetTravelRequest({
			vehicle: query.vehicle,
			createdAfter: query.createdAfter,
			createdBefore: query.createdBefore,
			status: query.status
		});
	}
}