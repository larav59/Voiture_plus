import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetAlarmRequest extends BaseRequest<GetAlarmRequest> {
	originId : number | null;
	occuredBefore : Date | null;
	occuredAfter : Date | null;
	typeId : number | null;

	constructor(fields?: Partial<GetAlarmRequest>) {
		super();
		this.originId = fields?.originId ?? null;
		this.occuredBefore = fields?.occuredBefore ?? null;
		this.occuredAfter = fields?.occuredAfter ?? null;
		this.typeId = fields?.typeId ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetAlarmRequest> {
		const validator = new Validator<GetAlarmRequest>(this);
		validator.field("originId").isNumber().greaterThan(0);
		validator.field("occuredBefore").isDate();
		validator.field("occuredAfter").isDate();
		validator.field("typeId").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetAlarmRequest {
		const queryString = req.query;
		queryString.originId = queryString.originId !== undefined ? parseInt(queryString.originId) : undefined;
		queryString.occuredBefore = queryString.occuredBefore !== undefined ? new Date(queryString.occuredBefore) : undefined;
		queryString.occuredAfter = queryString.occuredAfter !== undefined ? new Date(queryString.occuredAfter) : undefined;
		queryString.typeId = queryString.typeId !== undefined ? parseInt(queryString.typeId) : undefined;

		return new GetAlarmRequest({
			originId: queryString.originId,
			occuredBefore: queryString.occuredBefore,
			occuredAfter: queryString.occuredAfter,
			typeId: queryString.typeId,
		});
	}
}