import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetAlarmRequest extends BaseRequest<GetAlarmRequest> {
	originId : number | null;
	occuredBefore : Date | null;
	occuredAfter : Date | null;
	typeId : number | null;

	constructor(fields?: Partial<GetVehiclesRequest>) {
		super();
		this.originId = fields?.originId ?? null;
		this.occuredBefore = fields?.occuredBefore ?? null;
		this.occuredAfter = fields?.occuredAfter ?? null;
		this.typeId = fields?.typeId ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetVehiclesRequest> {
		const validator = new Validator<GetVehiclesRequest>(this);
		validator.field("originId").isNumber().greaterThan(0);
		validator.field("occuredBefore").isDate();
		validator.field("occuredAfter").isDate();
		validator.field("typeId").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetVehiclesRequest {
		const queryString = req.query;
		queryString.id = queryString.id !== undefined ? parseInt(queryString.id) : undefined;
		queryString.name = queryString.name !== undefined ? String(queryString.name) : undefined;

		return new GetVehiclesRequest({
			originId: queryString.originId,
			occuredBefore: queryString.occuredBefore,
			occuredAfter: queryString.occuredAfter,
			typeId: queryString.typeId,
		});
	}
}