import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateNodeRequest extends BaseRequest<UpdateNodeRequest> {
	id : number;
	name : string ;
	type : number ;
	positionX : number ;
	positionY : number ;
	offsetX : number ;
	offsetY : number ;
	isPointOfInterest : boolean ;

	constructor(fields?: Partial<UpdateNodeRequest>) {
		super();
		this.id = fields?.id !;
		this.positionX = fields?.positionX !;
		this.positionY = fields?.positionY !;
		this.offsetX = fields?.offsetX !;
		this.offsetY = fields?.offsetY !;
		this.isPointOfInterest = fields?.isPointOfInterest !;
		this.name = fields?.name !;
		this.type = fields?.type !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateNodeRequest> {
		const validator = new Validator<UpdateNodeRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("isPointOfInterest").isBoolean();
		validator.field("positionX").isNumber();
		validator.field("positionY").isNumber();
		validator.field("offsetX").isNumber();
		validator.field("offsetY").isNumber()
		validator.field("name").minLength(3).maxLength(255);
		validator.field("type").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateNodeRequest {
		const params = req.params
		const body = req.query
		return new UpdateNodeRequest({
			id: Number(params.id),
			positionX : Number(body.positionX),
			positionY : Number(body.positionY),
			offsetX : Number(body.offsetX),
			offsetY : Number(body.offsetY),
			isPointOfInterest: Boolean(body.isPointOfInterest),
			name: body.name,
			type: Number(body.type)
		});
	}
}