import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetTravelRequest extends BaseRequest<GetTravelRequest> {
	vehicleId : number | null;
	createdAfter : Date | null;
	createdBefore : Date | null;
	status : string | null;

	constructor(fields?: Partial<GetTravelRequest>) {
		super();
		this.vehicleId = fields?.vehicleId ?? null;
		this.createdAfter = fields?.createdAfter ?? null;
		this.createdBefore = fields?.createdBefore ?? null;
		this.status = fields?.status ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetTravelRequest> {
		const validator = new Validator<GetTravelRequest>(this);
		validator.field("vehicleId").isNumber().greaterThan(0);
		validator.field("createdAfter").isDate();
		validator.field("createdBefore").isDate();
		return validator;
	}

	static fromRequest(req: any): GetTravelRequest {
		const query = req.query;
		return new GetTravelRequest({
			vehicleId: Number(query.vehicleId) ? Number(query.vehicleId) : null,
			createdAfter: query.createdAfter ? new Date(query.createdAfter) : null,
			createdBefore: query.createdBefore ? new Date(query.createdBefore) : null,
			status: query.status ?? null
		});
	}
}