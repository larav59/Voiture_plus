import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  PrimaryGeneratedColumn,
} from "typeorm";
import { AlarmsTypes } from "./AlarmsTypes";
import { Origins } from "./Origins";

@Index("alarm_type_id", ["alarmTypeId"], {})
@Index("origin_id", ["originId"], {})
@Entity("ALARMS", { schema: "supervision" })
export class Alarms {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("timestamp", { name: "created_at", nullable: true })
  public createdAt!: Date | null;

  @Column("varchar", { name: "description", nullable: true, length: 255 })
  public description!: string | null;

  @Column("int", { name: "alarm_type_id", nullable: true })
  public alarmTypeId!: number | null;

  @Column("int", { name: "origin_id", nullable: true })
  public originId!: number | null;

  @ManyToOne(() => AlarmsTypes, (alarmsTypes) => alarmsTypes.alarms, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "alarm_type_id", referencedColumnName: "id" }])
  public alarmType!: AlarmsTypes;

  @ManyToOne(() => Origins, (origins) => origins.alarms, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "origin_id", referencedColumnName: "id" }])
  public origin!: Origins;

  public constructor(init?: Partial<Alarms>) {
    Object.assign(this, init);
  }
}
